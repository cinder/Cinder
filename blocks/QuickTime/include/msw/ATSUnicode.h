/*
     File:       ATSUnicode.h
 
     Contains:   Public interfaces for Apple Type Services for Unicode Imaging
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

#ifndef __ATSLAYOUTTYPES__
#include <ATSLayoutTypes.h>
#endif

#ifndef __FONTS__
#include <Fonts.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __SFNTTYPES__
#include <SFNTTypes.h>
#endif

#ifndef __SFNTLAYOUTTYPES__
#include <SFNTLayoutTypes.h>
#endif

#ifndef __ATSTYPES__
#include <ATSTypes.h>
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

/********************************/
/*  Types and related constants */
/********************************/
/*
   UniCharArrayHandle and UniCharArrayOffset are defined in the
   TextCommon interface file.
*/

/*
    ATSUTextMeasurement is specific to ATSUI in that measurement
    quantities are in fractional Fixed format instead of shorts
    used in QuickDraw Text.  This provides exact outline metrics and
    line specifications such as line width, ascent, descent, and so on.  
*/
typedef Fixed                           ATSUTextMeasurement;
/*
    ATSUFontID indicates a particular font family and face.  
    ATSUFontID's are not guaranteed to remain constant across 
    reboots.  Clients should use the font's unique name to 
    get a font token to store in documents which is 
    guaranteed to remain constant across reboots. 
*/
typedef FMFont                          ATSUFontID;
/*
    ATSUFontFeatureType and ATSUFontFeatureSelector are used 
    to identify font features.  
*/
typedef UInt16                          ATSUFontFeatureType;
typedef UInt16                          ATSUFontFeatureSelector;
/*
    ATSUFontVariationAxis and ATSUFontVariationValue are used 
    in connection with font variations.  
*/
typedef FourCharCode                    ATSUFontVariationAxis;
typedef Fixed                           ATSUFontVariationValue;
/*
    ATSUTextLayout is used to store the attribute information 
    associated with a contiguous block of UniChar's (UInt16's) 
    in memory.  It's typed to be an opaque structure.  
*/
typedef struct OpaqueATSUTextLayout*    ATSUTextLayout;
/*
    ATSUStyle is used to store a set of individual attributes, 
    font features, and font variations.  It's typed to be 
    an opaque structure.  
*/
typedef struct OpaqueATSUStyle*         ATSUStyle;
/*
    ATSUFontFallbacks is used to store the desired font fallback 
    list and associated fallback cache information.  It's typed
    to be an opaque structure.  
*/
typedef struct OpaqueATSUFontFallbacks*  ATSUFontFallbacks;
/*
    ATSUAttributeTag is used to indicate the particular type 
    of attribute under consideration:  font, size, color, 
    and so on.  
    Each style run may have at most one attribute with a 
    given ATSUAttributeTag (i.e., a style run can't have 
    more than one font or size) but may have none.  
    Note: Apple reserves values 0 to 65,535 (0 to 0x0000FFFF).
    ATSUI clients may create their own tags with any other value.
*/
typedef UInt32 ATSUAttributeTag;
enum {
                                        /*    Layout and Line Control Attribute Tags*/
  kATSULineWidthTag             = 1L,   /*    Type:       ATSUTextMeasurement >= 0*/
                                        /*    Default value: 0*/
  kATSULineRotationTag          = 2L,   /*    Type:       Fixed (fixed value in degrees in right-handed coordinate system)*/
                                        /*    Default value: 0*/
  kATSULineDirectionTag         = 3L,   /*    Type:       Boolean; values 0 or 1 (see below for value identities)*/
                                        /*    Default value: GetSysDirection() ? kATSURightToLeftBaseDirection : kATSULeftToRightBaseDirection*/
  kATSULineJustificationFactorTag = 4L, /*    Type:       Fract between 0 and 1*/
                                        /*    Default value: kATSUNoJustification*/
  kATSULineFlushFactorTag       = 5L,   /*    Type:       Fract between 0 and 1 */
                                        /*    Default value: kATSUStartAlignment*/
  kATSULineBaselineValuesTag    = 6L,   /*    Type:       BslnBaselineRecord*/
                                        /*    Default value: All zeros.  Calculated from other style attributes (e.g., font and point size)*/
  kATSULineLayoutOptionsTag     = 7L,   /*    Type:       ATSLineLayoutOptions (see ATSLayoutTypes.h)*/
                                        /*    Default value: kATSLineNoLayoutOptions - other options listed in ATSLayoutTypes.h*/
  kATSULineAscentTag            = 8L,   /*    Type:       ATSUTextMeasurement >= 0*/
                                        /*    Default value: Maximum typographical ascent of all fonts used on line or textLayout*/
  kATSULineDescentTag           = 9L,   /*    Type:       ATSUTextMeasurement >= 0*/
                                        /*    Default value: Maximum typographical descent+leading of all fonts used on line or textLayout*/
  kATSULineLangRegionTag        = 10L,  /*    Type:       RegionCode - region values listed in script.h interface file*/
                                        /*    Default value: kTextRegionDontCare*/
  kATSULineTextLocatorTag       = 11L,  /*    Type:       TextBreakLocatorRef*/
                                        /*    Default value: NULL - set Region derived locator or the default Text Utilities locator*/
  kATSULineTruncationTag        = 12L,  /*    Type:       ATSULineTruncation*/
                                        /*    Default value: kATSUTruncateNone                                                       */
  kATSULineFontFallbacksTag     = 13L,  /*    Type:       ATSUFontFallbacks*/
                                        /*    Default value: globally set font fallbacks using the ATSUSetFontFallbacks                                                     */
  kATSULayoutOperationOverrideTag = 15L, /*    Type:       ATSULayoutOperationOverrideSpecifier (MacOS 10.2 or later) */
                                        /*    Default value:     NULL*/
  kATSUMaxLineTag               = 16L,  /*    This is just for convenience - the upper limit of the ATSUTextLayout attribute tags*/
                                        /* Obsolete layout tags */
  kATSULineLanguageTag          = 10L,  /*    Use kATSULineLangRegionTag           */
                                        /* Special layout tags */
  kATSUCGContextTag             = 32767L, /*    Type:       CGContextRef (MacOS X systems only)*/
                                        /*    Default value: NULL*/
                                        /*    Run Style Attribute Tags*/
                                        /* QuickDraw compatibility tags */
  kATSUQDBoldfaceTag            = 256L, /*    Type:       Boolean    */
                                        /*    Default value: false*/
  kATSUQDItalicTag              = 257L, /*    Type:       Boolean       */
                                        /*    Default value: false*/
  kATSUQDUnderlineTag           = 258L, /*    Type:       Boolean    */
                                        /*    Default value: false*/
  kATSUQDCondensedTag           = 259L, /*    Type:       Boolean    */
                                        /*    Default value: false*/
  kATSUQDExtendedTag            = 260L, /*    Type:       Boolean    */
                                        /*    Default value: false*/
                                        /* Common run tags */
  kATSUFontTag                  = 261L, /*    Type:       ATSUFontID */
                                        /*    Default value: GetScriptVariable( smSystemScript, smScriptAppFond )*/
  kATSUSizeTag                  = 262L, /*    Type:       Fixed  */
                                        /*    Default value: GetScriptVariable( smSystemScript, smScriptAppFondSize )    */
  kATSUColorTag                 = 263L, /*    Type:       RGBColor*/
                                        /*    Default value: (0, 0, 0)*/
                                        /*    Less common run tags */
  kATSULangRegionTag            = 264L, /*    Type:       RegionCode - region values listed in script.h interface file*/
                                        /*    Default value: GetScriptManagerVariable( smRegionCode )*/
  kATSUVerticalCharacterTag     = 265L, /*    Type:       ATSUVerticalCharacterType  */
                                        /*    Default value: kATSUStronglyHorizontal*/
  kATSUImposeWidthTag           = 266L, /*    Type:       ATSUTextMeasurement >= 0*/
                                        /*    Default value: kATSUseGlyphAdvance - all glyphs use their own font defined advance widths*/
  kATSUBeforeWithStreamShiftTag = 267L, /*    Type:       Fixed*/
                                        /*    Default value: 0*/
  kATSUAfterWithStreamShiftTag  = 268L, /*    Type:       Fixed*/
                                        /*    Default value: 0*/
  kATSUCrossStreamShiftTag      = 269L, /*    Type:       Fixed*/
                                        /*    Default value: 0*/
  kATSUTrackingTag              = 270L, /*    Type:       Fixed*/
                                        /*    Default value: kATSNoTracking*/
  kATSUHangingInhibitFactorTag  = 271L, /*    Type:       Fract between 0 and 1*/
                                        /*    Default value: 0*/
  kATSUKerningInhibitFactorTag  = 272L, /*    Type:       Fract between 0 and 1*/
                                        /*    Default value: 0*/
  kATSUDecompositionFactorTag   = 273L, /*    Type:       Fixed (-1.0 -> 1.0)*/
                                        /*    Default value: 0*/
  kATSUBaselineClassTag         = 274L, /*    Type:       BslnBaselineClass  (see SFNTLayoutTypes.h)*/
                                        /*    Default value: kBSLNRomanBaseline - set to kBSLNNoBaselineOverride to use intrinsic baselines*/
  kATSUPriorityJustOverrideTag  = 275L, /*    Type:       ATSJustPriorityWidthDeltaOverrides (see ATSLayoutTypes.h)*/
                                        /*    Default value: all zeros*/
  kATSUNoLigatureSplitTag       = 276L, /*    Type:       Boolean*/
                                        /*    Default value: false - ligatures and compound characters have divisable components.*/
  kATSUNoCaretAngleTag          = 277L, /*    Type:       Boolean*/
                                        /*    Default value: false - use the character's angularity to determine its boundaries*/
  kATSUSuppressCrossKerningTag  = 278L, /*    Type:       Boolean*/
                                        /*    Default value: false - do not suppress automatic cross kerning (defined by font)*/
  kATSUNoOpticalAlignmentTag    = 279L, /*    Type:       Boolean*/
                                        /*    Default value: false - do not suppress character's automatic optical positional alignment*/
  kATSUForceHangingTag          = 280L, /*    Type:       Boolean*/
                                        /*    Default value: false - do not force the character's to hang beyond the line boundaries*/
  kATSUNoSpecialJustificationTag = 281L, /*    Type:       Boolean*/
                                        /*    Default value: false - perform post-compensation justification if needed*/
  kATSUStyleTextLocatorTag      = 282L, /*    Type:       TextBreakLocatorRef*/
                                        /*    Default value: NULL - region derived locator or the default Text Utilities locator*/
  kATSUStyleRenderingOptionsTag = 283L, /*    Type:       ATSStyleRenderingOptions (see ATSLayoutTypes.h)*/
                                        /*    Default value: kATSStyleNoOptions - other options listed in ATSLayoutTypes.h*/
  kATSUAscentTag                = 284L, /*    Type:       ATSUTextMeasurement >= 0  (MacOS 10.2 or later)*/
                                        /*    Default value: Ascent value of style's font with current pointSize*/
  kATSUDescentTag               = 285L, /*    Type:       ATSUTextMeasurement >= 0  (MacOS 10.2 or later)*/
                                        /*    Default value: Descent value of style's font with current pointSize*/
  kATSULeadingTag               = 286L, /*    Type:       ATSUTextMeasurement >= 0  (MacOS 10.2 or later)*/
                                        /*    Default value: Leading value of style's font with current pointSize (usually value 0)*/
  kATSUGlyphSelectorTag         = 287L, /*    Type:       ATSUGlyphSelector  (MacOS 10.2 or later)*/
                                        /*    Default value: 0 - use the glyphs derived by ATSUI layout*/
  kATSURGBAlphaColorTag         = 288L, /*    Type:       ATSURGBAlphaColor  (MacOS 10.2 or later)*/
                                        /*    Default value: (0, 0, 0, 1)*/
  kATSUFontMatrixTag            = 289L, /*    Type:       CGAffineTransform  (MacOS 10.2 or later)*/
                                        /*    Default value: [1, 0, 0, 1, 0, 0] ==> CGAffineTransformIdentity*/
  kATSUMaxStyleTag              = 290L, /*    This is just for convenience - the upper limit of the ATSUStyle attribute tags */
                                        /* obsolete style tags */
  kATSULanguageTag              = 264L, /*    use kATSULangRegionTag               */
                                        /* max */
  kATSUMaxATSUITagValue         = 65535L /*    This is the maximum Apple ATSUI reserved tag value.  Client defined tags must be larger.*/
};

/*
    ATSUAttributeValuePtr is used to provide generic access to
    storage of attribute values, which vary in size.
    ConstATSUAttributeValuePtr is a pointer to a const attribute value.
*/
typedef void *                          ATSUAttributeValuePtr;
typedef const void *                    ConstATSUAttributeValuePtr;
/*
    ATSUAttributeInfo is used to provide a tag/size pairing.  
    This makes it possible to provide the client information   
    about all the attributes for a given range of text.  This   
    structure is only used to return to the client information   
    about a complete set of attributes.  An array of   
    ATSUAttributeInfos is passed as a parameter so that the   
    client can find out what attributes are set and what their   
    individual sizes are; with that information, they can then   
    query about the values of the attributes they're interested   
    in.  Because arrays of ATSUAttributeInfos are used as parameters   
    to functions, they have to be of a fixed size, hence the   
    value is not included in the structure.  
*/
struct ATSUAttributeInfo {
  ATSUAttributeTag    fTag;
  ByteCount           fValueSize;
};
typedef struct ATSUAttributeInfo        ATSUAttributeInfo;
/*
    ATSUCaret contains the complete information needed to render a
    caret.  fX and fY is the position of one of the caret's ends
    relative to the origin position of the line the caret belongs.
    fDeltaX and fDeltaY is the position of the caret's other end.
    Hence, to draw a caret, simply call MoveTo(fX, fY) followed by
    LineTo(fDeltaX, fDeltaY) or equivalent.  The ATSUCaret will
    contain the positions needed to draw carets on angled lines
    and reflect angled carets and leading/trailing split caret
    appearances.
*/
struct ATSUCaret {
  Fixed               fX;
  Fixed               fY;
  Fixed               fDeltaX;
  Fixed               fDeltaY;
};
typedef struct ATSUCaret                ATSUCaret;
/*
    ATSUCursorMovementType is used to indicate how much to move
    the cursor when using the ATSUI cusor movement routines. 
    Note that kATSUByCharacterCluster is only available in Mac OS X
    and in CarbonLib versions 1.3 and later.
*/
typedef UInt16 ATSUCursorMovementType;
enum {
  kATSUByCharacter              = 0,
  kATSUByTypographicCluster     = 1,    /* clusters based on characters or ligatures*/
  kATSUByWord                   = 2,
  kATSUByCharacterCluster       = 3,    /* clusters based on characters only*/
  kATSUByCluster                = 1     /* obsolete name for kATSUByTypographicCluster*/
};

/*
    ATSULineTruncation is for identifying where truncation will occur when
    using a ATSUTextLayout with the ATSULineTruncation layout attribute.
*/
typedef UInt32 ATSULineTruncation;
enum {
  kATSUTruncateNone             = 0,    /* truncation specification - add to any desired truncFeat bit options listed below*/
  kATSUTruncateStart            = 1,    /* for instance, kATSUTruncateEnd with kATSUTruncFeatNoSquishing is value 0x0000000A*/
  kATSUTruncateEnd              = 2,
  kATSUTruncateMiddle           = 3,
  kATSUTruncateSpecificationMask = 0x00000007, /* these bits are reserved for the truncation specification (0 - 7)*/
                                        /* the following bit-field options can be added to the chosen truncation specification*/
  kATSUTruncFeatNoSquishing     = 0x00000008 /* if specified, do not perform any negative justification in lieu of truncation*/
};

/*
    ATSUVerticalCharacterType currently can take two values 
    and is used to indicate whether text is to be laid out 
    as vertical glyphs or horizontal glyphs.  
*/
typedef UInt16 ATSUVerticalCharacterType;
enum {
  kATSUStronglyHorizontal       = 0,
  kATSUStronglyVertical         = 1
};

/*
    ATSUStyleComparison is an enumeration with four values, 
    and is used by ATSUCompareStyles() to indicate if the
    first style parameter contains as a proper subset, is
    equal to, or is contained by the second style parameter.
*/
typedef UInt16 ATSUStyleComparison;
enum {
  kATSUStyleUnequal             = 0,
  kATSUStyleContains            = 1,
  kATSUStyleEquals              = 2,
  kATSUStyleContainedBy         = 3
};

/*
    ATSUFontFallbackMethod type defines the method by which ATSUI will try to
    find an appropriate font for a character if the assigned font does not
    contain the needed glyph(s) to represent it.  This affects ATSUMatchFontsToText
    and font selection during layout and drawing when ATSUSetTransientFontMatching
    is set ON.
*/
typedef UInt16 ATSUFontFallbackMethod;
enum {
  kATSUDefaultFontFallbacks     = 0,
  kATSULastResortOnlyFallback   = 1,
  kATSUSequentialFallbacksPreferred = 2,
  kATSUSequentialFallbacksExclusive = 3
};

/*
    ATSUTabType type defines the characteristic of ATSUI tabs.
    A Left tab type specifies that the left side of affected text is to be
    maintained flush against the tab stop.  A Right tab type specifies that
    the right side of affected text is to be maintained flush against the
    tab stop.  A Center tab type specifies that the affected text centered
    about the tab stop.
*/

typedef UInt16                          ATSUTabType;
enum {
  kATSULeftTab                  = 0,
  kATSUCenterTab                = 1,
  kATSURightTab                 = 2,
  kATSUNumberTabTypes           = 3
};

/*
    ATSUTab specifies the position and type of tab stop
    to be applied to a ATSUTextLayout set through the ATSUI routine
    ATSUSetTabArray and returned through ATSUGetTabArray.
*/
struct ATSUTab {
  ATSUTextMeasurement  tabPosition;
  ATSUTabType         tabType;
};
typedef struct ATSUTab                  ATSUTab;
/*
    GlyphCollection types represent the specific character collection.  If
    the value is zero, kGlyphCollectionGID, then this indicates that the
    glyph value represents the actual glyphID of a specific font.
*/

typedef UInt16                          GlyphCollection;
enum {
  kGlyphCollectionGID           = 0,
  kGlyphCollectionAdobeCNS1     = 1,
  kGlyphCollectionAdobeGB1      = 2,
  kGlyphCollectionAdobeJapan1   = 3,
  kGlyphCollectionAdobeJapan2   = 4,
  kGlyphCollectionAdobeKorea1   = 5,
  kGlyphCollectionUnspecified   = 0xFF
};

/*
    ATSUGlyphSelector can direct ATSUI to use a specific glyph instead of the one that 
    ATSUI normally derives.  The glyph can be specified either as a glyphID (specific
    to the font used) or CID from a specfic collection defined by the collection entry.
*/
struct ATSUGlyphSelector {
  GlyphCollection     collection;             /* kGlyphCollectionXXX enum*/
  GlyphID             glyphID;                /* GID (when collection==0) or CID*/
};
typedef struct ATSUGlyphSelector        ATSUGlyphSelector;
#if CALL_NOT_IN_CARBON
/*
    ATSUMemoryCallbacks is a union struct that allows the ATSUI 
    client to specify a specific heap for ATSUI use or allocation
    callbacks of which ATSUI is to use each time ATSUI performs a
    memory operation (alloc, grow, free).
*/
typedef CALLBACK_API_C( void *, ATSUCustomAllocFunc )(void *refCon, ByteCount howMuch);
typedef CALLBACK_API_C( void , ATSUCustomFreeFunc )(void *refCon, void *doomedBlock);
typedef CALLBACK_API_C( void *, ATSUCustomGrowFunc )(void *refCon, void *oldBlock, ByteCount oldSize, ByteCount newSize);
union ATSUMemoryCallbacks {
  struct {
    ATSUCustomAllocFunc  Alloc;
    ATSUCustomFreeFunc  Free;
    ATSUCustomGrowFunc  Grow;
    void *              memoryRefCon;
  }                       callbacks;

  THz                 heapToUse;
};
typedef union ATSUMemoryCallbacks       ATSUMemoryCallbacks;
/*
    ATSUHeapSpec provides the ATSUI client a means of specifying the heap
    from which ATSUI should allocate its dynamic memory or specifying
    that ATSUI should use the memory callback provided by the client.
*/
typedef UInt16 ATSUHeapSpec;
enum {
  kATSUUseCurrentHeap           = 0,
  kATSUUseAppHeap               = 1,
  kATSUUseSpecificHeap          = 2,
  kATSUUseCallbacks             = 3
};


/*
    ATSUMemorySetting is used to store the results from a ATSUSetMemoryAlloc
    or a ATSUGetCurrentMemorySetting call.  It can also be used to change the
    current ATSUMemorySetting by passing it into the ATSUSetCurrentMemorySetting
    call.
*/
typedef struct OpaqueATSUMemorySetting*  ATSUMemorySetting;
#endif  /* CALL_NOT_IN_CARBON */

/* Types for ATSUI Low Level API */
struct ATSUGlyphInfo {
  GlyphID             glyphID;
  UInt16              reserved;
  UInt32              layoutFlags;
  UniCharArrayOffset  charIndex;
  ATSUStyle           style;
  Float32             deltaY;
  Float32             idealX;
  SInt16              screenX;
  SInt16              caretX;
};
typedef struct ATSUGlyphInfo            ATSUGlyphInfo;
struct ATSUGlyphInfoArray {
  ATSUTextLayout      layout;
  ItemCount           numGlyphs;
  ATSUGlyphInfo       glyphs[1];
};
typedef struct ATSUGlyphInfoArray       ATSUGlyphInfoArray;
/********************/
/*  Other constants */
/********************/
/* Line direction types (used for kATSULineDirectionTag values) */
enum {
  kATSULeftToRightBaseDirection = 0,    /*    Impose left-to-right or top-to-bottom dominant direction */
  kATSURightToLeftBaseDirection = 1     /*    Impose right-to-left or bottom-to-top dominant direction */
};

/* LineFlushFactor convenience defined values */
#define kATSUStartAlignment         ((Fract) 0x00000000L)
#define kATSUEndAlignment           ((Fract) 0x40000000L)
#define kATSUCenterAlignment        ((Fract) 0x20000000L)
/* LineJustificationFactor convenience defined values */
#define kATSUNoJustification        ((Fract) 0x00000000L)
#define kATSUFullJustification      ((Fract) 0x40000000L)
/* Other constants  */
enum {
  kATSUInvalidFontID            = 0
};


enum {
  kATSUUseLineControlWidth      = 0x7FFFFFFF
};


enum {
  kATSUNoSelector               = 0x0000FFFF
};


enum {
  kATSUUseGrafPortPenLoc        = (unsigned long)0xFFFFFFFF,
  kATSUClearAll                 = (unsigned long)0xFFFFFFFF
};


enum {
  kATSUFromTextBeginning        = (unsigned long)0xFFFFFFFF,
  kATSUToTextEnd                = (unsigned long)0xFFFFFFFF
};



/****************/
/*  Functions   */
/****************/

/* ---------------------------------------------------------------------------- */
/*  Clipboard support, flattened style version 0                                */
/* ---------------------------------------------------------------------------- */
/*
    Warning: ATSUCopyToHandle & ATSUPasteFromHandle have been deprecated
    Use their replacements, ATSUFlattenStyleRunsToStream and , instead if available or
    perform your own flattening
*/
/*
 *  ATSUCopyToHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCopyToHandle(
  ATSUStyle   iStyle,
  Handle      oStyleHandle);


/*
 *  ATSUPasteFromHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUPasteFromHandle(
  ATSUStyle   iStyle,
  Handle      iStyleHandle);


/* ---------------------------------------------------------------------------- */
/*  Memory allocation specification functions (not in Carbon)                   */
/* ---------------------------------------------------------------------------- */
#if CALL_NOT_IN_CARBON
/*
 *  ATSUCreateMemorySetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
ATSUCreateMemorySetting(
  ATSUHeapSpec           iHeapSpec,
  ATSUMemoryCallbacks *  iMemoryCallbacks,
  ATSUMemorySetting *    oMemorySetting);


/*
 *  ATSUSetCurrentMemorySetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
ATSUSetCurrentMemorySetting(ATSUMemorySetting iMemorySetting);


/*
 *  ATSUGetCurrentMemorySetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( ATSUMemorySetting )
ATSUGetCurrentMemorySetting(void);


/*
 *  ATSUDisposeMemorySetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
ATSUDisposeMemorySetting(ATSUMemorySetting iMemorySetting);


/* ---------------------------------------------------------------------------- */
/*  Font fallback object functions                                              */
/* ---------------------------------------------------------------------------- */
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  ATSUCreateFontFallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateFontFallbacks(ATSUFontFallbacks * oFontFallback);


/*
 *  ATSUDisposeFontFallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
ATSUDisposeFontFallbacks(ATSUFontFallbacks iFontFallbacks);


/*
 *  ATSUSetObjFontFallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetObjFontFallbacks(
  ATSUFontFallbacks        iFontFallbacks,
  ItemCount                iFontFallbacksCount,
  const ATSUFontID         iFonts[],
  ATSUFontFallbackMethod   iFontFallbackMethod);


/*
 *  ATSUGetObjFontFallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetObjFontFallbacks(
  ATSUFontFallbacks         iFontFallbacks,
  ItemCount                 iMaxFontFallbacksCount,
  ATSUFontID                oFonts[],
  ATSUFontFallbackMethod *  oFontFallbackMethod,
  ItemCount *               oActualFallbacksCount);


/* ---------------------------------------------------------------------------- */
/*  Basic style functions                                                       */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCreateStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateStyle(ATSUStyle * oStyle);


/*
 *  ATSUCreateAndCopyStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateAndCopyStyle(
  ATSUStyle    iStyle,
  ATSUStyle *  oStyle);


/*
 *  ATSUDisposeStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUDisposeStyle(ATSUStyle iStyle);


/*
 *  ATSUSetStyleRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetStyleRefCon(
  ATSUStyle   iStyle,
  UInt32      iRefCon);


/*
 *  ATSUGetStyleRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetStyleRefCon(
  ATSUStyle   iStyle,
  UInt32 *    oRefCon);


/* ---------------------------------------------------------------------------- */
/*  Style comparison                                                            */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCompareStyles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCompareStyles(
  ATSUStyle              iFirstStyle,
  ATSUStyle              iSecondStyle,
  ATSUStyleComparison *  oComparison);


/* ---------------------------------------------------------------------------- */
/*  Attribute manipulations                                                     */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCopyAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCopyAttributes(
  ATSUStyle   iSourceStyle,
  ATSUStyle   iDestinationStyle);


/*
 *  ATSUOverwriteAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUOverwriteAttributes(
  ATSUStyle   iSourceStyle,
  ATSUStyle   iDestinationStyle);


/*
 *  ATSUUnderwriteAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUUnderwriteAttributes(
  ATSUStyle   iSourceStyle,
  ATSUStyle   iDestinationStyle);


/* ---------------------------------------------------------------------------- */
/*  Empty styles                                                                */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUClearStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearStyle(ATSUStyle iStyle);


/*
 *  ATSUStyleIsEmpty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUStyleIsEmpty(
  ATSUStyle   iStyle,
  Boolean *   oIsClear);


/* ---------------------------------------------------------------------------- */
/*  Get and set attributes                                                      */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCalculateBaselineDeltas()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCalculateBaselineDeltas(
  ATSUStyle            iStyle,
  BslnBaselineClass    iBaselineClass,
  BslnBaselineRecord   oBaselineDeltas);


/*
 *  ATSUSetAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetAttributes(
  ATSUStyle                     iStyle,
  ItemCount                     iAttributeCount,
  const ATSUAttributeTag        iTag[],
  const ByteCount               iValueSize[],
  const ATSUAttributeValuePtr   iValue[]);


/*
 *  ATSUGetAttribute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetAttribute(
  ATSUStyle               iStyle,
  ATSUAttributeTag        iTag,
  ByteCount               iExpectedValueSize,
  ATSUAttributeValuePtr   oValue,
  ByteCount *             oActualValueSize);


/*
 *  ATSUGetAllAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetAllAttributes(
  ATSUStyle           iStyle,
  ATSUAttributeInfo   oAttributeInfoArray[],
  ItemCount           iTagValuePairArraySize,
  ItemCount *         oTagValuePairCount);


/*
 *  ATSUClearAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearAttributes(
  ATSUStyle                iStyle,
  ItemCount                iTagCount,
  const ATSUAttributeTag   iTag[]);


/* ---------------------------------------------------------------------------- */
/*  Font features                                                               */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUSetFontFeatures()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetFontFeatures(
  ATSUStyle                       iStyle,
  ItemCount                       iFeatureCount,
  const ATSUFontFeatureType       iType[],
  const ATSUFontFeatureSelector   iSelector[]);


/*
 *  ATSUGetFontFeature()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontFeature(
  ATSUStyle                  iStyle,
  ItemCount                  iFeatureIndex,
  ATSUFontFeatureType *      oFeatureType,
  ATSUFontFeatureSelector *  oFeatureSelector);


/*
 *  ATSUGetAllFontFeatures()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetAllFontFeatures(
  ATSUStyle                 iStyle,
  ItemCount                 iMaximumFeatureCount,
  ATSUFontFeatureType       oFeatureType[],
  ATSUFontFeatureSelector   oFeatureSelector[],
  ItemCount *               oActualFeatureCount);


/*
 *  ATSUClearFontFeatures()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearFontFeatures(
  ATSUStyle                       iStyle,
  ItemCount                       iFeatureCount,
  const ATSUFontFeatureType       iType[],
  const ATSUFontFeatureSelector   iSelector[]);


/* ---------------------------------------------------------------------------- */
/*  Font variations                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUSetVariations()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetVariations(
  ATSUStyle                      iStyle,
  ItemCount                      iVariationCount,
  const ATSUFontVariationAxis    iAxes[],
  const ATSUFontVariationValue   iValue[]);


/*
 *  ATSUGetFontVariationValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontVariationValue(
  ATSUStyle                 iStyle,
  ATSUFontVariationAxis     iFontVariationAxis,
  ATSUFontVariationValue *  oFontVariationValue);


/*
 *  ATSUGetAllFontVariations()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetAllFontVariations(
  ATSUStyle                iStyle,
  ItemCount                iVariationCount,
  ATSUFontVariationAxis    oVariationAxes[],
  ATSUFontVariationValue   oFontVariationValues[],
  ItemCount *              oActualVariationCount);


/*
 *  ATSUClearFontVariations()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearFontVariations(
  ATSUStyle                     iStyle,
  ItemCount                     iAxisCount,
  const ATSUFontVariationAxis   iAxis[]);


/* ---------------------------------------------------------------------------- */
/*  Basic text-layout functions                                                 */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCreateTextLayout()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateTextLayout(ATSUTextLayout * oTextLayout);


/*
 *  ATSUCreateAndCopyTextLayout()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateAndCopyTextLayout(
  ATSUTextLayout    iTextLayout,
  ATSUTextLayout *  oTextLayout);


/*
 *  ATSUCreateTextLayoutWithTextPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateTextLayoutWithTextPtr(
  ConstUniCharArrayPtr   iText,
  UniCharArrayOffset     iTextOffset,
  UniCharCount           iTextLength,
  UniCharCount           iTextTotalLength,
  ItemCount              iNumberOfRuns,
  const UniCharCount     iRunLengths[],
  ATSUStyle              iStyles[],
  ATSUTextLayout *       oTextLayout);


/*
 *  ATSUCreateTextLayoutWithTextHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCreateTextLayoutWithTextHandle(
  UniCharArrayHandle   iText,
  UniCharArrayOffset   iTextOffset,
  UniCharCount         iTextLength,
  UniCharCount         iTextTotalLength,
  ItemCount            iNumberOfRuns,
  const UniCharCount   iRunLengths[],
  ATSUStyle            iStyles[],
  ATSUTextLayout *     oTextLayout);


/*
 *  ATSUClearLayoutCache()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearLayoutCache(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iLineStart);


/*
 *  ATSUDisposeTextLayout()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUDisposeTextLayout(ATSUTextLayout iTextLayout);


/*
 *  ATSUSetTextLayoutRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetTextLayoutRefCon(
  ATSUTextLayout   iTextLayout,
  UInt32           iRefCon);


/*
 *  ATSUGetTextLayoutRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetTextLayoutRefCon(
  ATSUTextLayout   iTextLayout,
  UInt32 *         oRefCon);


/* ---------------------------------------------------------------------------- */
/*  Glyph bounds retrieval                                                      */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUGetGlyphBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetGlyphBounds(
  ATSUTextLayout        iTextLayout,
  ATSUTextMeasurement   iTextBasePointX,
  ATSUTextMeasurement   iTextBasePointY,
  UniCharArrayOffset    iBoundsCharStart,
  UniCharCount          iBoundsCharLength,
  UInt16                iTypeOfBounds,
  ItemCount             iMaxNumberOfBounds,
  ATSTrapezoid          oGlyphBounds[],
  ItemCount *           oActualNumberOfBounds);


/* ---------------------------------------------------------------------------- */
/*  Idle processing                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUIdle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUIdle(ATSUTextLayout iTextLayout);


/* ---------------------------------------------------------------------------- */
/*  Text location                                                               */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUSetTextPointerLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetTextPointerLocation(
  ATSUTextLayout         iTextLayout,
  ConstUniCharArrayPtr   iText,
  UniCharArrayOffset     iTextOffset,
  UniCharCount           iTextLength,
  UniCharCount           iTextTotalLength);


/*
 *  ATSUSetTextHandleLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetTextHandleLocation(
  ATSUTextLayout       iTextLayout,
  UniCharArrayHandle   iText,
  UniCharArrayOffset   iTextOffset,
  UniCharCount         iTextLength,
  UniCharCount         iTextTotalLength);


/*
 *  ATSUGetTextLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetTextLocation(
  ATSUTextLayout        iTextLayout,
  void **               oText,
  Boolean *             oTextIsStoredInHandle,
  UniCharArrayOffset *  oOffset,
  UniCharCount *        oTextLength,
  UniCharCount *        oTextTotalLength);


/* ---------------------------------------------------------------------------- */
/*  Text manipulation                                                           */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUTextDeleted()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUTextDeleted(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iDeletedRangeStart,
  UniCharCount         iDeletedRangeLength);


/*
 *  ATSUTextInserted()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUTextInserted(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iInsertionLocation,
  UniCharCount         iInsertionLength);


/*
 *  ATSUTextMoved()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUTextMoved(
  ATSUTextLayout         iTextLayout,
  ConstUniCharArrayPtr   iNewLocation);


/* ---------------------------------------------------------------------------- */
/*  Layout controls                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCopyLayoutControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCopyLayoutControls(
  ATSUTextLayout   iSourceTextLayout,
  ATSUTextLayout   iDestTextLayout);


/*
 *  ATSUSetLayoutControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetLayoutControls(
  ATSUTextLayout                iTextLayout,
  ItemCount                     iAttributeCount,
  const ATSUAttributeTag        iTag[],
  const ByteCount               iValueSize[],
  const ATSUAttributeValuePtr   iValue[]);


/*
 *  ATSUGetLayoutControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetLayoutControl(
  ATSUTextLayout          iTextLayout,
  ATSUAttributeTag        iTag,
  ByteCount               iExpectedValueSize,
  ATSUAttributeValuePtr   oValue,
  ByteCount *             oActualValueSize);


/*
 *  ATSUGetAllLayoutControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetAllLayoutControls(
  ATSUTextLayout      iTextLayout,
  ATSUAttributeInfo   oAttributeInfoArray[],
  ItemCount           iTagValuePairArraySize,
  ItemCount *         oTagValuePairCount);


/*
 *  ATSUClearLayoutControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearLayoutControls(
  ATSUTextLayout           iTextLayout,
  ItemCount                iTagCount,
  const ATSUAttributeTag   iTag[]);


/* ---------------------------------------------------------------------------- */
/*  Single line layout controls                                                 */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCopyLineControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCopyLineControls(
  ATSUTextLayout       iSourceTextLayout,
  UniCharArrayOffset   iSourceLineStart,
  ATSUTextLayout       iDestTextLayout,
  UniCharArrayOffset   iDestLineStart);


/*
 *  ATSUSetLineControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetLineControls(
  ATSUTextLayout                iTextLayout,
  UniCharArrayOffset            iLineStart,
  ItemCount                     iAttributeCount,
  const ATSUAttributeTag        iTag[],
  const ByteCount               iValueSize[],
  const ATSUAttributeValuePtr   iValue[]);


/*
 *  ATSUGetLineControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetLineControl(
  ATSUTextLayout          iTextLayout,
  UniCharArrayOffset      iLineStart,
  ATSUAttributeTag        iTag,
  ByteCount               iExpectedValueSize,
  ATSUAttributeValuePtr   oValue,
  ByteCount *             oActualValueSize);


/*
 *  ATSUGetAllLineControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetAllLineControls(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iLineStart,
  ATSUAttributeInfo    oAttributeInfoArray[],
  ItemCount            iTagValuePairArraySize,
  ItemCount *          oTagValuePairCount);


/*
 *  ATSUClearLineControls()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearLineControls(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iLineStart,
  ItemCount                iTagCount,
  const ATSUAttributeTag   iTag[]);


/* ---------------------------------------------------------------------------- */
/*  Style run processing                                                        */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUSetRunStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetRunStyle(
  ATSUTextLayout       iTextLayout,
  ATSUStyle            iStyle,
  UniCharArrayOffset   iRunStart,
  UniCharCount         iRunLength);


/*
 *  ATSUGetRunStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetRunStyle(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iOffset,
  ATSUStyle *           oStyle,
  UniCharArrayOffset *  oRunStart,
  UniCharCount *        oRunLength);


/*
 *  ATSUGetContinuousAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetContinuousAttributes(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iOffset,
  UniCharCount         iLength,
  ATSUStyle            oStyle);


/* ---------------------------------------------------------------------------- */
/*  Drawing and measuring                                                       */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUDrawText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUDrawText(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iLineOffset,
  UniCharCount          iLineLength,
  ATSUTextMeasurement   iLocationX,
  ATSUTextMeasurement   iLocationY);


/*
    Warning: ATSUMeasureText has been deprecated
    Use its replacement, ATSUGetUnjustifiedBounds, instead if available
*/
/*
 *  ATSUMeasureText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUMeasureText(
  ATSUTextLayout         iTextLayout,
  UniCharArrayOffset     iLineStart,
  UniCharCount           iLineLength,
  ATSUTextMeasurement *  oTextBefore,
  ATSUTextMeasurement *  oTextAfter,
  ATSUTextMeasurement *  oAscent,
  ATSUTextMeasurement *  oDescent);


/*
 *  ATSUGetUnjustifiedBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetUnjustifiedBounds(
  ATSUTextLayout         iTextLayout,
  UniCharArrayOffset     iLineStart,
  UniCharCount           iLineLength,
  ATSUTextMeasurement *  oTextBefore,
  ATSUTextMeasurement *  oTextAfter,
  ATSUTextMeasurement *  oAscent,
  ATSUTextMeasurement *  oDescent);


/*
 *  ATSUMeasureTextImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUMeasureTextImage(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iLineOffset,
  UniCharCount          iLineLength,
  ATSUTextMeasurement   iLocationX,
  ATSUTextMeasurement   iLocationY,
  Rect *                oTextImageRect);


/* ---------------------------------------------------------------------------- */
/*  Highlighting                                                                */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUHighlightText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUHighlightText(
  ATSUTextLayout        iTextLayout,
  ATSUTextMeasurement   iTextBasePointX,
  ATSUTextMeasurement   iTextBasePointY,
  UniCharArrayOffset    iHighlightStart,
  UniCharCount          iHighlightLength);


/*
 *  ATSUUnhighlightText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUUnhighlightText(
  ATSUTextLayout        iTextLayout,
  ATSUTextMeasurement   iTextBasePointX,
  ATSUTextMeasurement   iTextBasePointY,
  UniCharArrayOffset    iHighlightStart,
  UniCharCount          iHighlightLength);


/*
 *  ATSUGetTextHighlight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetTextHighlight(
  ATSUTextLayout        iTextLayout,
  ATSUTextMeasurement   iTextBasePointX,
  ATSUTextMeasurement   iTextBasePointY,
  UniCharArrayOffset    iHighlightStart,
  UniCharCount          iHighlightLength,
  RgnHandle             oHighlightRegion);


/*
 *  ATSUHighlightInactiveText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUHighlightInactiveText(
  ATSUTextLayout        iTextLayout,
  ATSUTextMeasurement   iTextBasePointX,
  ATSUTextMeasurement   iTextBasePointY,
  UniCharArrayOffset    iHighlightStart,
  UniCharCount          iHighlightLength);


/* ---------------------------------------------------------------------------- */
/*  Hit-testing                                                                 */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUPositionToOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUPositionToOffset(
  ATSUTextLayout        iTextLayout,
  ATSUTextMeasurement   iLocationX,
  ATSUTextMeasurement   iLocationY,
  UniCharArrayOffset *  ioPrimaryOffset,
  Boolean *             oIsLeading,
  UniCharArrayOffset *  oSecondaryOffset);


/*
 *  ATSUOffsetToPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUOffsetToPosition(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iOffset,
  Boolean              iIsLeading,
  ATSUCaret *          oMainCaret,
  ATSUCaret *          oSecondCaret,
  Boolean *            oCaretIsSplit);


/*
 *  ATSUPositionToCursorOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.2.2 and later
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUPositionToCursorOffset(
  ATSUTextLayout           iTextLayout,
  ATSUTextMeasurement      iLocationX,
  ATSUTextMeasurement      iLocationY,
  ATSUCursorMovementType   iMovementType,
  UniCharArrayOffset *     ioPrimaryOffset,
  Boolean *                oIsLeading,
  UniCharArrayOffset *     oSecondaryOffset);


/*
 *  ATSUOffsetToCursorPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.2.2 and later
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUOffsetToCursorPosition(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iOffset,
  Boolean                  iIsLeading,
  ATSUCursorMovementType   iMovementType,
  ATSUCaret *              oMainCaret,
  ATSUCaret *              oSecondCaret,
  Boolean *                oCaretIsSplit);


/* ---------------------------------------------------------------------------- */
/*  Cursor movement                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUNextCursorPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUNextCursorPosition(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iOldOffset,
  ATSUCursorMovementType   iMovementType,
  UniCharArrayOffset *     oNewOffset);


/*
 *  ATSUPreviousCursorPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUPreviousCursorPosition(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iOldOffset,
  ATSUCursorMovementType   iMovementType,
  UniCharArrayOffset *     oNewOffset);


/*
 *  ATSURightwardCursorPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSURightwardCursorPosition(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iOldOffset,
  ATSUCursorMovementType   iMovementType,
  UniCharArrayOffset *     oNewOffset);


/*
 *  ATSULeftwardCursorPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSULeftwardCursorPosition(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iOldOffset,
  ATSUCursorMovementType   iMovementType,
  UniCharArrayOffset *     oNewOffset);


/* ---------------------------------------------------------------------------- */
/*  Line breaking                                                               */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUBatchBreakLines()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUBatchBreakLines(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iRangeStart,
  UniCharCount          iRangeLength,
  ATSUTextMeasurement   iLineWidth,
  ItemCount *           oBreakCount);


/*
 *  ATSUBreakLine()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUBreakLine(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iLineStart,
  ATSUTextMeasurement   iLineWidth,
  Boolean               iUseAsSoftLineBreak,
  UniCharArrayOffset *  oLineBreak);


/*
 *  ATSUSetSoftLineBreak()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetSoftLineBreak(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iLineBreak);


/*
 *  ATSUGetSoftLineBreaks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetSoftLineBreaks(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iRangeStart,
  UniCharCount         iRangeLength,
  ItemCount            iMaximumBreaks,
  UniCharArrayOffset   oBreaks[],
  ItemCount *          oBreakCount);


/*
 *  ATSUClearSoftLineBreaks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUClearSoftLineBreaks(
  ATSUTextLayout       iTextLayout,
  UniCharArrayOffset   iRangeStart,
  UniCharCount         iRangeLength);


/* ---------------------------------------------------------------------------- */
/*  Tab support                                                                 */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUSetTabArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetTabArray(
  ATSUTextLayout   iTextLayout,
  const ATSUTab    iTabs[],
  ItemCount        iTabCount);


/*
 *  ATSUGetTabArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetTabArray(
  ATSUTextLayout   iTextLayout,
  ItemCount        iMaxTabCount,
  ATSUTab          oTabs[],
  ItemCount *      oTabCount);


/* ---------------------------------------------------------------------------- */
/*  Font matching                                                               */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUSetFontFallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetFontFallbacks(
  ItemCount                iFontFallbacksCount,
  const ATSUFontID         iFontIDs[],
  ATSUFontFallbackMethod   iFontFallbackMethod);


/*
 *  ATSUGetFontFallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontFallbacks(
  ItemCount                 iMaxFontFallbacksCount,
  ATSUFontID                oFontIDs[],
  ATSUFontFallbackMethod *  oFontFallbackMethod,
  ItemCount *               oActualFallbacksCount);


/*
 *  ATSUMatchFontsToText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUMatchFontsToText(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iTextStart,
  UniCharCount          iTextLength,
  ATSUFontID *          oFontID,
  UniCharArrayOffset *  oChangedOffset,
  UniCharCount *        oChangedLength);


/*
 *  ATSUSetTransientFontMatching()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUSetTransientFontMatching(
  ATSUTextLayout   iTextLayout,
  Boolean          iTransientFontMatching);


/*
 *  ATSUGetTransientFontMatching()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetTransientFontMatching(
  ATSUTextLayout   iTextLayout,
  Boolean *        oTransientFontMatching);


/* ---------------------------------------------------------------------------- */
/*  Font ID's                                                                   */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUFontCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUFontCount(ItemCount * oFontCount);


/*
 *  ATSUGetFontIDs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontIDs(
  ATSUFontID   oFontIDs[],
  ItemCount    iArraySize,
  ItemCount *  oFontCount);


/*
 *  ATSUFONDtoFontID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUFONDtoFontID(
  short         iFONDNumber,
  Style         iFONDStyle,
  ATSUFontID *  oFontID);


/*
 *  ATSUFontIDtoFOND()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUFontIDtoFOND(
  ATSUFontID   iFontID,
  short *      oFONDNumber,
  Style *      oFONDStyle);


/* ---------------------------------------------------------------------------- */
/*  Font names                                                                  */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCountFontNames()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCountFontNames(
  ATSUFontID   iFontID,
  ItemCount *  oFontNameCount);


/*
 *  ATSUGetIndFontName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetIndFontName(
  ATSUFontID          iFontID,
  ItemCount           iFontNameIndex,
  ByteCount           iMaximumNameLength,
  Ptr                 oName,
  ByteCount *         oActualNameLength,
  FontNameCode *      oFontNameCode,
  FontPlatformCode *  oFontNamePlatform,
  FontScriptCode *    oFontNameScript,
  FontLanguageCode *  oFontNameLanguage);


/*
 *  ATSUFindFontName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUFindFontName(
  ATSUFontID         iFontID,
  FontNameCode       iFontNameCode,
  FontPlatformCode   iFontNamePlatform,
  FontScriptCode     iFontNameScript,
  FontLanguageCode   iFontNameLanguage,
  ByteCount          iMaximumNameLength,
  Ptr                oName,
  ByteCount *        oActualNameLength,
  ItemCount *        oFontNameIndex);


/*
 *  ATSUFindFontFromName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUFindFontFromName(
  Ptr                iName,
  ByteCount          iNameLength,
  FontNameCode       iFontNameCode,
  FontPlatformCode   iFontNamePlatform,
  FontScriptCode     iFontNameScript,
  FontLanguageCode   iFontNameLanguage,
  ATSUFontID *       oFontID);


/* ---------------------------------------------------------------------------- */
/*  Font features                                                               */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCountFontFeatureTypes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCountFontFeatureTypes(
  ATSUFontID   iFontID,
  ItemCount *  oTypeCount);


/*
 *  ATSUCountFontFeatureSelectors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCountFontFeatureSelectors(
  ATSUFontID            iFontID,
  ATSUFontFeatureType   iType,
  ItemCount *           oSelectorCount);


/*
 *  ATSUGetFontFeatureTypes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontFeatureTypes(
  ATSUFontID            iFontID,
  ItemCount             iMaximumTypes,
  ATSUFontFeatureType   oTypes[],
  ItemCount *           oActualTypeCount);


/*
 *  ATSUGetFontFeatureSelectors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontFeatureSelectors(
  ATSUFontID                iFontID,
  ATSUFontFeatureType       iType,
  ItemCount                 iMaximumSelectors,
  ATSUFontFeatureSelector   oSelectors[],
  Boolean                   oSelectorIsOnByDefault[],
  ItemCount *               oActualSelectorCount,
  Boolean *                 oIsMutuallyExclusive);


/*
 *  ATSUGetFontFeatureNameCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontFeatureNameCode(
  ATSUFontID                iFontID,
  ATSUFontFeatureType       iType,
  ATSUFontFeatureSelector   iSelector,
  FontNameCode *            oNameCode);


/* ---------------------------------------------------------------------------- */
/*  Font tracking value & names                                                 */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCountFontTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCountFontTracking(
  ATSUFontID                  iFontID,
  ATSUVerticalCharacterType   iCharacterOrientation,
  ItemCount *                 oTrackingCount);


/*
 *  ATSUGetIndFontTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetIndFontTracking(
  ATSUFontID                  iFontID,
  ATSUVerticalCharacterType   iCharacterOrientation,
  ItemCount                   iTrackIndex,
  Fixed *                     oFontTrackingValue,
  FontNameCode *              oNameCode);


/* ---------------------------------------------------------------------------- */
/*  Font variations                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCountFontVariations()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCountFontVariations(
  ATSUFontID   iFontID,
  ItemCount *  oVariationCount);


/*
 *  ATSUGetIndFontVariation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetIndFontVariation(
  ATSUFontID                iFontID,
  ItemCount                 iVariationIndex,
  ATSUFontVariationAxis *   oATSUFontVariationAxis,
  ATSUFontVariationValue *  oMinimumValue,
  ATSUFontVariationValue *  oMaximumValue,
  ATSUFontVariationValue *  oDefaultValue);


/*
 *  ATSUGetFontVariationNameCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontVariationNameCode(
  ATSUFontID              iFontID,
  ATSUFontVariationAxis   iAxis,
  FontNameCode *          oNameCode);


/* ---------------------------------------------------------------------------- */
/*  Font Instances                                                              */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUCountFontInstances()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUCountFontInstances(
  ATSUFontID   iFontID,
  ItemCount *  oInstances);


/*
 *  ATSUGetFontInstance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontInstance(
  ATSUFontID               iFontID,
  ItemCount                iFontInstanceIndex,
  ItemCount                iMaximumVariations,
  ATSUFontVariationAxis    oAxes[],
  ATSUFontVariationValue   oValues[],
  ItemCount *              oActualVariationCount);


/*
 *  ATSUGetFontInstanceNameCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetFontInstanceNameCode(
  ATSUFontID      iFontID,
  ItemCount       iInstanceIndex,
  FontNameCode *  oNameCode);



/********************************************************************************/
/* ATSUI Low-Level API.  For more low-level calls, see ATSUnicodeDirectAccess.h */
/********************************************************************************/
/* ---------------------------------------------------------------------------- */
/* GlyphInfo access                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUGetGlyphInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetGlyphInfo(
  ATSUTextLayout        iTextLayout,
  UniCharArrayOffset    iLineStart,
  UniCharCount          iLineLength,
  ByteCount *           ioBufferSize,
  ATSUGlyphInfoArray *  oGlyphInfoPtr);


/*
 *  ATSUDrawGlyphInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUDrawGlyphInfo(
  ATSUGlyphInfoArray *  iGlyphInfoArray,
  Float32Point          iLocation);


/* ---------------------------------------------------------------------------- */
/* Font Data Access                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUGlyphGetIdealMetrics()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGlyphGetIdealMetrics(
  ATSUStyle              iATSUStyle,
  ItemCount              iNumOfGlyphs,
  GlyphID                iGlyphIDs[],
  ByteOffset             iInputOffset,
  ATSGlyphIdealMetrics   oIdealMetrics[]);


/*
 *  ATSUGetNativeCurveType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGetNativeCurveType(
  ATSUStyle       iATSUStyle,
  ATSCurveType *  oCurveType);


/* ---------------------------------------------------------------------------- */
/* Device specific routines                                                     */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUGlyphGetScreenMetrics()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGlyphGetScreenMetrics(
  ATSUStyle               iATSUStyle,
  ItemCount               iNumOfGlyphs,
  GlyphID                 iGlyphIDs[],
  ByteOffset              iInputOffset,
  Boolean                 iForcingAntiAlias,
  Boolean                 iAntiAliasSwitch,
  ATSGlyphScreenMetrics   oScreenMetrics[]);


/* ---------------------------------------------------------------------------- */
/* ATSUGlyphGetQuadraticPaths callbacks                                         */
/* ---------------------------------------------------------------------------- */
typedef CALLBACK_API( OSStatus , ATSQuadraticLineProcPtr )(const Float32Point *pt1, const Float32Point *pt2, void *callBackDataPtr);
typedef CALLBACK_API( OSStatus , ATSQuadraticCurveProcPtr )(const Float32Point *pt1, const Float32Point *controlPt, const Float32Point *pt2, void *callBackDataPtr);
typedef CALLBACK_API( OSStatus , ATSQuadraticNewPathProcPtr )(void * callBackDataPtr);
typedef CALLBACK_API( OSStatus , ATSQuadraticClosePathProcPtr )(void * callBackDataPtr);
typedef TVECTOR_UPP_TYPE(ATSQuadraticLineProcPtr)               ATSQuadraticLineUPP;
typedef TVECTOR_UPP_TYPE(ATSQuadraticCurveProcPtr)              ATSQuadraticCurveUPP;
typedef TVECTOR_UPP_TYPE(ATSQuadraticNewPathProcPtr)            ATSQuadraticNewPathUPP;
typedef TVECTOR_UPP_TYPE(ATSQuadraticClosePathProcPtr)          ATSQuadraticClosePathUPP;
/*
 *  NewATSQuadraticLineUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSQuadraticLineUPP )
NewATSQuadraticLineUPP(ATSQuadraticLineProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSQuadraticLineProcInfo = 0x00000FF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSQuadraticLineUPP) NewATSQuadraticLineUPP(ATSQuadraticLineProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSQuadraticLineUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewATSQuadraticCurveUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSQuadraticCurveUPP )
NewATSQuadraticCurveUPP(ATSQuadraticCurveProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSQuadraticCurveProcInfo = 0x00003FF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSQuadraticCurveUPP) NewATSQuadraticCurveUPP(ATSQuadraticCurveProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSQuadraticCurveUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewATSQuadraticNewPathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSQuadraticNewPathUPP )
NewATSQuadraticNewPathUPP(ATSQuadraticNewPathProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSQuadraticNewPathProcInfo = 0x000000F0 };  /* pascal 4_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSQuadraticNewPathUPP) NewATSQuadraticNewPathUPP(ATSQuadraticNewPathProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSQuadraticNewPathUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewATSQuadraticClosePathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSQuadraticClosePathUPP )
NewATSQuadraticClosePathUPP(ATSQuadraticClosePathProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSQuadraticClosePathProcInfo = 0x000000F0 };  /* pascal 4_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSQuadraticClosePathUPP) NewATSQuadraticClosePathUPP(ATSQuadraticClosePathProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSQuadraticClosePathUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeATSQuadraticLineUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSQuadraticLineUPP(ATSQuadraticLineUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSQuadraticLineUPP(ATSQuadraticLineUPP) {}
  #else
      #define DisposeATSQuadraticLineUPP(userUPP)
  #endif
#endif

/*
 *  DisposeATSQuadraticCurveUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSQuadraticCurveUPP(ATSQuadraticCurveUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSQuadraticCurveUPP(ATSQuadraticCurveUPP) {}
  #else
      #define DisposeATSQuadraticCurveUPP(userUPP)
  #endif
#endif

/*
 *  DisposeATSQuadraticNewPathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSQuadraticNewPathUPP(ATSQuadraticNewPathUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSQuadraticNewPathUPP(ATSQuadraticNewPathUPP) {}
  #else
      #define DisposeATSQuadraticNewPathUPP(userUPP)
  #endif
#endif

/*
 *  DisposeATSQuadraticClosePathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSQuadraticClosePathUPP(ATSQuadraticClosePathUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSQuadraticClosePathUPP(ATSQuadraticClosePathUPP) {}
  #else
      #define DisposeATSQuadraticClosePathUPP(userUPP)
  #endif
#endif

/*
 *  InvokeATSQuadraticLineUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSQuadraticLineUPP(
  const Float32Point *  pt1,
  const Float32Point *  pt2,
  void *                callBackDataPtr,
  ATSQuadraticLineUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSQuadraticLineUPP(const Float32Point * pt1, const Float32Point * pt2, void * callBackDataPtr, ATSQuadraticLineUPP userUPP) { return (*userUPP)(pt1, pt2, callBackDataPtr); }
  #else
      #define InvokeATSQuadraticLineUPP(pt1, pt2, callBackDataPtr, userUPP) (*userUPP)(pt1, pt2, callBackDataPtr)
  #endif
#endif

/*
 *  InvokeATSQuadraticCurveUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSQuadraticCurveUPP(
  const Float32Point *  pt1,
  const Float32Point *  controlPt,
  const Float32Point *  pt2,
  void *                callBackDataPtr,
  ATSQuadraticCurveUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSQuadraticCurveUPP(const Float32Point * pt1, const Float32Point * controlPt, const Float32Point * pt2, void * callBackDataPtr, ATSQuadraticCurveUPP userUPP) { return (*userUPP)(pt1, controlPt, pt2, callBackDataPtr); }
  #else
      #define InvokeATSQuadraticCurveUPP(pt1, controlPt, pt2, callBackDataPtr, userUPP) (*userUPP)(pt1, controlPt, pt2, callBackDataPtr)
  #endif
#endif

/*
 *  InvokeATSQuadraticNewPathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSQuadraticNewPathUPP(
  void *                  callBackDataPtr,
  ATSQuadraticNewPathUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSQuadraticNewPathUPP(void * callBackDataPtr, ATSQuadraticNewPathUPP userUPP) { return (*userUPP)(callBackDataPtr); }
  #else
      #define InvokeATSQuadraticNewPathUPP(callBackDataPtr, userUPP) (*userUPP)(callBackDataPtr)
  #endif
#endif

/*
 *  InvokeATSQuadraticClosePathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSQuadraticClosePathUPP(
  void *                    callBackDataPtr,
  ATSQuadraticClosePathUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSQuadraticClosePathUPP(void * callBackDataPtr, ATSQuadraticClosePathUPP userUPP) { return (*userUPP)(callBackDataPtr); }
  #else
      #define InvokeATSQuadraticClosePathUPP(callBackDataPtr, userUPP) (*userUPP)(callBackDataPtr)
  #endif
#endif

/*
 *  ATSUGlyphGetQuadraticPaths()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGlyphGetQuadraticPaths(
  ATSUStyle                  iATSUStyle,
  GlyphID                    iGlyphID,
  ATSQuadraticNewPathUPP     iNewPathProc,
  ATSQuadraticLineUPP        iLineProc,
  ATSQuadraticCurveUPP       iCurveProc,
  ATSQuadraticClosePathUPP   iClosePathProc,
  void *                     iCallbackDataPtr,
  OSStatus *                 oCallbackResult);


/* ATSUGlyphGetCubicPaths callbacks */
typedef CALLBACK_API( OSStatus , ATSCubicMoveToProcPtr )(const Float32Point *pt, void *callBackDataPtr);
typedef CALLBACK_API( OSStatus , ATSCubicLineToProcPtr )(const Float32Point *pt, void *callBackDataPtr);
typedef CALLBACK_API( OSStatus , ATSCubicCurveToProcPtr )(const Float32Point *pt1, const Float32Point *pt2, const Float32Point *pt3, void *callBackDataPtr);
typedef CALLBACK_API( OSStatus , ATSCubicClosePathProcPtr )(void * callBackDataPtr);
typedef TVECTOR_UPP_TYPE(ATSCubicMoveToProcPtr)                 ATSCubicMoveToUPP;
typedef TVECTOR_UPP_TYPE(ATSCubicLineToProcPtr)                 ATSCubicLineToUPP;
typedef TVECTOR_UPP_TYPE(ATSCubicCurveToProcPtr)                ATSCubicCurveToUPP;
typedef TVECTOR_UPP_TYPE(ATSCubicClosePathProcPtr)              ATSCubicClosePathUPP;
/*
 *  NewATSCubicMoveToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSCubicMoveToUPP )
NewATSCubicMoveToUPP(ATSCubicMoveToProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSCubicMoveToProcInfo = 0x000003F0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSCubicMoveToUPP) NewATSCubicMoveToUPP(ATSCubicMoveToProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSCubicMoveToUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewATSCubicLineToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSCubicLineToUPP )
NewATSCubicLineToUPP(ATSCubicLineToProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSCubicLineToProcInfo = 0x000003F0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSCubicLineToUPP) NewATSCubicLineToUPP(ATSCubicLineToProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSCubicLineToUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewATSCubicCurveToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSCubicCurveToUPP )
NewATSCubicCurveToUPP(ATSCubicCurveToProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSCubicCurveToProcInfo = 0x00003FF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSCubicCurveToUPP) NewATSCubicCurveToUPP(ATSCubicCurveToProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSCubicCurveToUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewATSCubicClosePathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSCubicClosePathUPP )
NewATSCubicClosePathUPP(ATSCubicClosePathProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSCubicClosePathProcInfo = 0x000000F0 };  /* pascal 4_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSCubicClosePathUPP) NewATSCubicClosePathUPP(ATSCubicClosePathProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSCubicClosePathUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeATSCubicMoveToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSCubicMoveToUPP(ATSCubicMoveToUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSCubicMoveToUPP(ATSCubicMoveToUPP) {}
  #else
      #define DisposeATSCubicMoveToUPP(userUPP)
  #endif
#endif

/*
 *  DisposeATSCubicLineToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSCubicLineToUPP(ATSCubicLineToUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSCubicLineToUPP(ATSCubicLineToUPP) {}
  #else
      #define DisposeATSCubicLineToUPP(userUPP)
  #endif
#endif

/*
 *  DisposeATSCubicCurveToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSCubicCurveToUPP(ATSCubicCurveToUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSCubicCurveToUPP(ATSCubicCurveToUPP) {}
  #else
      #define DisposeATSCubicCurveToUPP(userUPP)
  #endif
#endif

/*
 *  DisposeATSCubicClosePathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeATSCubicClosePathUPP(ATSCubicClosePathUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSCubicClosePathUPP(ATSCubicClosePathUPP) {}
  #else
      #define DisposeATSCubicClosePathUPP(userUPP)
  #endif
#endif

/*
 *  InvokeATSCubicMoveToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSCubicMoveToUPP(
  const Float32Point *  pt,
  void *                callBackDataPtr,
  ATSCubicMoveToUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSCubicMoveToUPP(const Float32Point * pt, void * callBackDataPtr, ATSCubicMoveToUPP userUPP) { return (*userUPP)(pt, callBackDataPtr); }
  #else
      #define InvokeATSCubicMoveToUPP(pt, callBackDataPtr, userUPP) (*userUPP)(pt, callBackDataPtr)
  #endif
#endif

/*
 *  InvokeATSCubicLineToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSCubicLineToUPP(
  const Float32Point *  pt,
  void *                callBackDataPtr,
  ATSCubicLineToUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSCubicLineToUPP(const Float32Point * pt, void * callBackDataPtr, ATSCubicLineToUPP userUPP) { return (*userUPP)(pt, callBackDataPtr); }
  #else
      #define InvokeATSCubicLineToUPP(pt, callBackDataPtr, userUPP) (*userUPP)(pt, callBackDataPtr)
  #endif
#endif

/*
 *  InvokeATSCubicCurveToUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSCubicCurveToUPP(
  const Float32Point *  pt1,
  const Float32Point *  pt2,
  const Float32Point *  pt3,
  void *                callBackDataPtr,
  ATSCubicCurveToUPP    userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSCubicCurveToUPP(const Float32Point * pt1, const Float32Point * pt2, const Float32Point * pt3, void * callBackDataPtr, ATSCubicCurveToUPP userUPP) { return (*userUPP)(pt1, pt2, pt3, callBackDataPtr); }
  #else
      #define InvokeATSCubicCurveToUPP(pt1, pt2, pt3, callBackDataPtr, userUPP) (*userUPP)(pt1, pt2, pt3, callBackDataPtr)
  #endif
#endif

/*
 *  InvokeATSCubicClosePathUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSCubicClosePathUPP(
  void *                callBackDataPtr,
  ATSCubicClosePathUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSCubicClosePathUPP(void * callBackDataPtr, ATSCubicClosePathUPP userUPP) { return (*userUPP)(callBackDataPtr); }
  #else
      #define InvokeATSCubicClosePathUPP(callBackDataPtr, userUPP) (*userUPP)(callBackDataPtr)
  #endif
#endif

/*
 *  ATSUGlyphGetCubicPaths()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGlyphGetCubicPaths(
  ATSUStyle              iATSUStyle,
  GlyphID                iGlyphID,
  ATSCubicMoveToUPP      iMoveToProc,
  ATSCubicLineToUPP      iLineToProc,
  ATSCubicCurveToUPP     iCurveToProc,
  ATSCubicClosePathUPP   iClosePathProc,
  void *                 iCallbackDataPtr,
  OSStatus *             oCallbackResult);


/*
 *  ATSUGlyphGetCurvePaths()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ATSUnicodeLib 9.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSUGlyphGetCurvePaths(
  ATSUStyle         iATSUStyle,
  GlyphID           iGlyphID,
  ByteCount *       ioBufferSize,
  ATSUCurvePaths *  oPaths);




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
