/*
     File:       ATSLayoutTypes.h
 
     Contains:   Apple Type Services layout public structures and constants.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1994-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ATSLAYOUTTYPES__
#define __ATSLAYOUTTYPES__

#ifndef __MACTYPES__
#include <MacTypes.h>
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

/* --------------------------------------------------------------------------- */
/* CONSTANTS and related scalar types */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* Miscellaneous Constants */
/* --------------------------------------------------------------------------- */
enum {
  kATSUseGlyphAdvance           = 0x7FFFFFFF, /* assignment to use natural glyph advance value */
  kATSUseLineHeight             = 0x7FFFFFFF, /* assignment to use natural line ascent/descent values */
  kATSNoTracking                = (long)0x80000000 /* negativeInfinity */
};

/* --------------------------------------------------------------------------- */

/*
 *  Summary:
 *    These values are passed into the ATSUGetGlyphBounds function to
 *    indicate whether the width of the resulting typographic glyph
 *    bounds will be determined using the caret origin, glyph origin in
 *    device space, or glyph origin in fractional absolute positions
 */
enum {

  /*
   * Specifies that the width of the typographic glyph bounds will be
   * determined using the caret origin. The caret origin is halfway
   * between two characters.
   */
  kATSUseCaretOrigins           = 0,

  /*
   * Specifies that the width of the typographic glyph bounds will be
   * determined using the glyph origin in device space. This is useful
   * for adjusting text on the screen.
   */
  kATSUseDeviceOrigins          = 1,

  /*
   * Specifies that the width of the typographic glyph bounds will be
   * determined using the glyph origin in fractional absolute
   * positions, which are uncorrected for device display. This provides
   * the ideal position of laid-out text and is useful for scaling text
   * on the screen. This origin is also used to get the width of the
   * typographic bounding rectangle when you call ATSUMeasureText.
   */
  kATSUseFractionalOrigins      = 2,
  kATSUseOriginFlags            = 3
};

/* ---------------------------------------------------------------------------- */

/*
 *  ATSULayoutOperationSelector
 *  
 *  Summary:
 *    This is used to select which operations to override, or which
 *    operation is currently being run.
 */
typedef UInt32 ATSULayoutOperationSelector;
enum {

  /*
   * No Layout operation is currently selected.
   */
  kATSULayoutOperationNone      = 0x00000000,

  /*
   * Select the Justification operation.
   */
  kATSULayoutOperationJustification = 0x00000001,

  /*
   * Select the character morphing operation.
   */
  kATSULayoutOperationMorph     = 0x00000002,

  /*
   * Select the kerning adjustment operation.
   */
  kATSULayoutOperationKerningAdjustment = 0x00000004,

  /*
   * Select the baseline adjustment operation.
   */
  kATSULayoutOperationBaselineAdjustment = 0x00000008,

  /*
   * Select the tracking adjustment operation.
   */
  kATSULayoutOperationTrackingAdjustment = 0x00000010,

  /*
   * Select the period of time after ATSUI has finished all of it's
   * layout operations.
   */
  kATSULayoutOperationPostLayoutAdjustment = 0x00000020,
  kATSULayoutOperationAppleReserved = (unsigned long)0xFFFFFFC0
};

/* ---------------------------------------------------------------------------- */

/*
 *  ATSULayoutOperationCallbackStatus
 *  
 *  Summary:
 *    One of these must be returned by a
 *    ATSUDLayoutOperationOverrideUPP callback function in order to
 *    indicate ATSUI's status.
 */
typedef UInt32 ATSULayoutOperationCallbackStatus;
enum {

  /*
   * Return this if the callback function has totally handled the
   * operation which triggered the callback and does not need ATSUI to
   * run any further processing for the operation.
   */
  kATSULayoutOperationCallbackStatusHandled = 0x00000000,

  /*
   * Return this if the callback function has not totally handled the
   * operation which triggered the callback and needs ATSUI to run it's
   * own processing.
   */
  kATSULayoutOperationCallbackStatusContinue = 0x00000001
};

/* ---------------------------------------------------------------------------- */

/*
 *  ATSLineLayoutOptions
 *  
 *  Summary:
 *    ATSLineLayoutOptions are normally set in an ATSUTextLayout object
 *    via the kATSULineLayoutOptionsTag layout control attribute. They
 *    can also be set in an ATSLineLayoutParams structure and passed
 *    into the ATSLayoutText function for finer control over the line
 *    layout.
 */
typedef UInt32 ATSLineLayoutOptions;
enum {

  /*
   * No options specified.
   */
  kATSLineNoLayoutOptions       = 0x00000000,

  /*
   * This line option is no longer used.
   */
  kATSLineIsDisplayOnly         = 0x00000001, /* obsolete option*/

  /*
   * Specifies that no hangers to be formed on the line.
   */
  kATSLineHasNoHangers          = 0x00000002,

  /*
   * Specifies that no optical alignment to be performed on the line.
   */
  kATSLineHasNoOpticalAlignment = 0x00000004,

  /*
   * Specifies that space charcters should not be treated as hangers.
   */
  kATSLineKeepSpacesOutOfMargin = 0x00000008,

  /*
   * Specifies no post-compensation justification is to be performed.
   */
  kATSLineNoSpecialJustification = 0x00000010,

  /*
   * Specifies that if the line is the last of a paragraph, it will not
   * get justified.
   */
  kATSLineLastNoJustification   = 0x00000020,

  /*
   * Specifies that the displayed line glyphs will adjust for device
   * metrics.
   */
  kATSLineFractDisable          = 0x00000040,

  /*
   * Specifies that the carets at the ends of the line will be
   * guarenteed to be perpendicular to the baseline.
   */
  kATSLineImposeNoAngleForEnds  = 0x00000080,

  /*
   * Highlights for the line end characters will be extended to 0 and
   * the specified line width.
   */
  kATSLineFillOutToWidth        = 0x00000100,

  /*
   * Specifies that the tab character width will be automatically
   * adjusted to fit the specified line width.
   */
  kATSLineTabAdjustEnabled      = 0x00000200,

  /*
   * Specifies that any leading value specified by a font will be
   * ignored.
   */
  kATSLineIgnoreFontLeading     = 0x00000400,

  /*
   * Specifies that ATS produce antialiased glyph images despite system
   * preferences or CGContext settings.
   */
  kATSLineApplyAntiAliasing     = 0x00000800,

  /*
   * Specifies that ATS turn-off antialiasing glyph imaging despite
   * system preferences or CGContext settings (negates
   * kATSLineApplyAntiAliasing bit if set).
   */
  kATSLineNoAntiAliasing        = 0x00001000,

  /*
   * Specifies that if the line width is not sufficient to hold all its
   * glyphs, glyph positions are allowed to extend beyond the line's
   * assigned width so negative justification is not used.
   */
  kATSLineDisableNegativeJustification = 0x00002000,

  /*
   * Specifies that lines with any integer glyph positioning (due to
   * either any character non-antialiased or kATSLineFractDisable
   * specified), not automatically esthetically adjust individual
   * character positions while rendering to display.
   */
  kATSLineDisableAutoAdjustDisplayPos = 0x00004000,

  /*
   * Specifies that rendering be done through Quickdraw (default
   * rendering in ATSUI is through CoreGraphics on MacOSX).
   */
  kATSLineUseQDRendering        = 0x00008000,

  /*
   * Specifies that any Justification operations will not be run.
   */
  kATSLineDisableAllJustification = 0x00010000,

  /*
   * Specifies that any glyph morphing operations will not be run.
   */
  kATSLineDisableAllGlyphMorphing = 0x00020000,

  /*
   * Specifies that any kerning adjustment operations will not be run.
   */
  kATSLineDisableAllKerningAdjustments = 0x00040000,

  /*
   * Specifies that any baseline adjustment operations will not be run.
   */
  kATSLineDisableAllBaselineAdjustments = 0x00080000,

  /*
   * Specifies that any tracking adjustment operations will not be run.
   */
  kATSLineDisableAllTrackingAdjustments = 0x00100000,

  /*
   * Convenience constant for turning-off all adjustments.
   */
  kATSLineDisableAllLayoutOperations = kATSLineDisableAllJustification | kATSLineDisableAllGlyphMorphing | kATSLineDisableAllKerningAdjustments | kATSLineDisableAllBaselineAdjustments | kATSLineDisableAllTrackingAdjustments,

  /*
   * Specifies to optimize for displaying text only.  Note, rounded
   * device metrics will be used instead of fractional path metrics.
   */
  kATSLineUseDeviceMetrics      = 0x01000000,

  /*
   * These bits are reserved by Apple and will result in a invalid
   * value error if attemped to set. Obsolete constants:
   */
  kATSLineAppleReserved         = (unsigned long)0xFEE00000
};

/* --------------------------------------------------------------------------- */

/*
 *  ATSStyleRenderingOptions
 *  
 *  Summary:
 *    ATSStyleRenderingOptions are set in the ATSUStyle object via the
 *    attribute tag kATSUStyleRenderingOptions. They provide finer
 *    control over how the style is rendered.
 */
typedef UInt32 ATSStyleRenderingOptions;
enum {

  /*
   * No options specified.
   */
  kATSStyleNoOptions            = 0x00000000,

  /*
   * Specifies that ATS produce "unhinted" glyph outlines (default is
   * hinted glyph outlines).
   */
  kATSStyleNoHinting            = 0x00000001,

  /*
   * Specifies that ATS produce antialiased glyph images despite system
   * preferences or CGContext settings.
   */
  kATSStyleApplyAntiAliasing    = 0x00000002,

  /*
   * Specifies that ATS turn-off antialiasing glyph imaging despite
   * system preferences or CGContext settings (negates
   * kATSStyleApplyAntiAliasing bit if set).
   */
  kATSStyleNoAntiAliasing       = 0x00000004,

  /*
   * These bits are reserved by Apple and will result in a invalid
   * value error if attemped to set.
   */
  kATSStyleAppleReserved        = (unsigned long)0xFFFFFFF8,

  /*
   * (OBSOLETE) Specifies that ATS produce "hinted" glyph outlines (the
   * default behavior). THIS NAME IS OBSOLETE. DO NOT USE. It's only
   * left in for backwards compatibility.
   */
  kATSStyleApplyHints           = kATSStyleNoOptions
};

/* --------------------------------------------------------------------------- */



/*
 *  ATSGlyphInfoFlags
 *  
 *  Summary:
 *    ATSGlyphInfoFlags are set in the individual ATSLayoutRecord
 *    structures and apply only to the ATSGlyphRef in that structure.
 *    The are used by the layout engine to flag a glyph with specific
 *    properties.
 */
typedef UInt32 ATSGlyphInfoFlags;
enum {

  /*
   * These bits are Apple reserved and may result in an invalid value
   * error if attempted to set.
   */
  kATSGlyphInfoAppleReserved    = 0x1FFBFFE8,

  /*
   * The glyph attaches to another glyph.
   */
  kATSGlyphInfoIsAttachment     = (unsigned long)0x80000000,

  /*
   * The glyph can hang off left/top edge of line.
   */
  kATSGlyphInfoIsLTHanger       = 0x40000000,

  /*
   * The glyph can hang off right/bottom edge of line.
   */
  kATSGlyphInfoIsRBHanger       = 0x20000000,

  /*
   * The glyph is not really a glyph at all, but an end-marker designed
   * to allow the calculation of the previous glyph's advance.
   */
  kATSGlyphInfoTerminatorGlyph  = 0x00080000,

  /*
   * The glyph is a white space glyph.
   */
  kATSGlyphInfoIsWhiteSpace     = 0x00040000,

  /*
   * Glyph has a style specified imposed width (i.e. advance)
   */
  kATSGlyphInfoHasImposedWidth  = 0x00000010,

  /*
   * A three-bit mask, that can be used to get the size of the original
   * character that spawned this glyph. When a logical 'and' operation
   * with this mask and an ATSGlyphInfoFlags variable, it will yield
   * the size in bytes of the original character (0 - 7 bytes possible).
   */
  kATSGlyphInfoByteSizeMask     = 0x00000007
};

/* --------------------------------------------------------------------------- */
/* STRUCTURED TYPES and related constants */
/* --------------------------------------------------------------------------- */

/*
 *  ATSLayoutRecord
 *  
 *  Summary:
 *    The ATSLayoutRecord structure defines all the needed info for a
 *    single glyph during the layout process.  This struct must be
 *    declared as the first element of an enclosing glyph record struct
 *    defined by ATSUI DirectAccess clients.
 */
struct ATSLayoutRecord {

  /*
   * The glyph ID reference.
   */
  ATSGlyphRef         glyphID;

  /*
   * These flags describe the individual state of the glyph (see above).
   */
  ATSGlyphInfoFlags   flags;

  /*
   * The byte offset of the original character that spawned this glyph.
   */
  ByteCount           originalOffset;

  /*
   * This is the real position that the glyph sits.
   */
  Fixed               realPos;
};
typedef struct ATSLayoutRecord          ATSLayoutRecord;
/* --------------------------------------------------------------------------- */

/*
 *  ATSTrapezoid
 *  
 *  Summary:
 *    The ATSTrapezoid structure supplies a convenient container for
 *    glyph bounds in trapezoidal form.
 */
struct ATSTrapezoid {
  FixedPoint          upperLeft;
  FixedPoint          upperRight;
  FixedPoint          lowerRight;
  FixedPoint          lowerLeft;
};
typedef struct ATSTrapezoid             ATSTrapezoid;
/* --------------------------------------------------------------------------- */

/*
 *  ATSJustWidthDeltaEntryOverride
 *  
 *  Summary:
 *    The JustWidthDeltaEntryOverride structure specifies values for
 *    the grow and shrink case during justification, both on the left
 *    and on the right. It also contains flags.  This particular
 *    structure is used for passing justification overrides to LLC. For
 *    further sfnt resource 'just' table constants and structures, see
 *    SFNTLayoutTypes.h.
 */
struct ATSJustWidthDeltaEntryOverride {

  /*
   * ems AW can grow by at most on LT
   */
  Fixed               beforeGrowLimit;

  /*
   * ems AW can shrink by at most on LT
   */
  Fixed               beforeShrinkLimit;

  /*
   * ems AW can grow by at most on RB
   */
  Fixed               afterGrowLimit;

  /*
   * ems AW can shrink by at most on RB
   */
  Fixed               afterShrinkLimit;

  /*
   * flags controlling grow case
   */
  JustificationFlags  growFlags;

  /*
   * flags controlling shrink case
   */
  JustificationFlags  shrinkFlags;
};
typedef struct ATSJustWidthDeltaEntryOverride ATSJustWidthDeltaEntryOverride;
/* The JustPriorityOverrides type is an array of 4 width delta records, one per priority level override. */
typedef ATSJustWidthDeltaEntryOverride  ATSJustPriorityWidthDeltaOverrides[4];
/* ---------------------------------------------------------------------------- */

/*
 *  ATSULineRef
 *  
 *  Summary:
 *    A reference to a line that is being laid out. This is passed into
 *    the ATSUDirectLayoutOperationOverrideUPP callback function to be
 *    used by the ATSUDirectGetLayoutDataArrayPtrFromLineRef function.
 *    The only way to get a line ref is inside of the callback. The
 *    only time the line ref is valid is inside of the callback.
 */
typedef struct ATSGlyphVector*          ATSULineRef;
/* ---------------------------------------------------------------------------- */
/* DirectAccess Layout Callback Definitions                                     */
/* ---------------------------------------------------------------------------- */

/*
 *  ATSUDirectLayoutOperationOverrideProcPtr
 *  
 *  Summary:
 *    Callback definition for a low-level adjustment routine hook.
 *  
 *  Discussion:
 *    This callback can be set in an ATSUTextLayout object by setting
 *    the attribute tag kATSULayoutOperationOverrideUPP and passing in
 *    a ATSULayoutOperationOverrideSpecifier structure into
 *    ATSUSetLayoutAttribute. This callback will be called whenever an
 *    ATSUI call triggers a re-layout for each operation it is
 *    installed for. The operation that triggered the callback will be
 *    set in the iCurrentOperation parameter. The callback function
 *    defined by the developer is only required to do one thing: return
 *    it's status to ATSUI as to what it has done. This is done via the
 *    oCallbackStatus parameter. It needs to tell ATSUI if it had
 *    handled the layout operation or if it still needs ATSUI to run
 *    it's own processes. iOperationCallbackParameterPtr is there in
 *    case there are ever any ATSUDirectLayoutOperationSelector which
 *    require extra parameters to be passed into the callback function.
 *    It is currently unused and will always be set to NULL. iRefCon is
 *    the constant that is set in the ATSUTextLayout object that
 *    spawned the operation by the ATSUSetTextLayoutRefCon() API.
 *    Within the context of the callback itself, only a limited subset
 *    of ATSUI APIs may be called. Basically, only the APIs that have
 *    no chance of triggering a re-layout are allowed to be called. The
 *    reason for this restriction is to prevent runaway recursion. Most
 *    of the APIs that have "create", "get", or "copy" are safe. Any
 *    attempt to call one of the restricted APIs will result in an
 *    immediate return with the kATSUInvalidCallInsideCallbackErr
 *    error. ATSULayoutOperationSelector and
 *    ATSULayoutOperationCallbackStatus are defined in ATSLayoutTypes.i.
 */
typedef CALLBACK_API_C( OSStatus , ATSUDirectLayoutOperationOverrideProcPtr )(ATSULayoutOperationSelector iCurrentOperation, ATSULineRef iLineRef, UInt32 iRefCon, void *iOperationCallbackParameterPtr, ATSULayoutOperationCallbackStatus *oCallbackStatus);
typedef TVECTOR_UPP_TYPE(ATSUDirectLayoutOperationOverrideProcPtr)  ATSUDirectLayoutOperationOverrideUPP;
/*
 *  NewATSUDirectLayoutOperationOverrideUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( ATSUDirectLayoutOperationOverrideUPP )
NewATSUDirectLayoutOperationOverrideUPP(ATSUDirectLayoutOperationOverrideProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppATSUDirectLayoutOperationOverrideProcInfo = 0x0000FFF1 };  /* 4_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ATSUDirectLayoutOperationOverrideUPP) NewATSUDirectLayoutOperationOverrideUPP(ATSUDirectLayoutOperationOverrideProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewATSUDirectLayoutOperationOverrideUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeATSUDirectLayoutOperationOverrideUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
DisposeATSUDirectLayoutOperationOverrideUPP(ATSUDirectLayoutOperationOverrideUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeATSUDirectLayoutOperationOverrideUPP(ATSUDirectLayoutOperationOverrideUPP) {}
  #else
      #define DisposeATSUDirectLayoutOperationOverrideUPP(userUPP)
  #endif
#endif

/*
 *  InvokeATSUDirectLayoutOperationOverrideUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
InvokeATSUDirectLayoutOperationOverrideUPP(
  ATSULayoutOperationSelector           iCurrentOperation,
  ATSULineRef                           iLineRef,
  UInt32                                iRefCon,
  void *                                iOperationCallbackParameterPtr,
  ATSULayoutOperationCallbackStatus *   oCallbackStatus,
  ATSUDirectLayoutOperationOverrideUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeATSUDirectLayoutOperationOverrideUPP(ATSULayoutOperationSelector iCurrentOperation, ATSULineRef iLineRef, UInt32 iRefCon, void * iOperationCallbackParameterPtr, ATSULayoutOperationCallbackStatus * oCallbackStatus, ATSUDirectLayoutOperationOverrideUPP userUPP) { return (*userUPP)(iCurrentOperation, iLineRef, iRefCon, iOperationCallbackParameterPtr, oCallbackStatus); }
  #else
      #define InvokeATSUDirectLayoutOperationOverrideUPP(iCurrentOperation, iLineRef, iRefCon, iOperationCallbackParameterPtr, oCallbackStatus, userUPP) (*userUPP)(iCurrentOperation, iLineRef, iRefCon, iOperationCallbackParameterPtr, oCallbackStatus)
  #endif
#endif

/* ---------------------------------------------------------------------------- */

/*
 *  ATSULayoutOperationOverrideSpecifier
 *  
 *  Summary:
 *    This structure is used to install a callback for one or more
 *    ATSUI operations. To do this, simply passed one of these
 *    structure into the ATSUSetLayoutControls call with the
 *    kATSULayoutOperationOverrideUPP tag.
 */
struct ATSULayoutOperationOverrideSpecifier {


  /*
   * A bitfield containing the selector for the operations in which the
   * callback will be installed for.
   */
  ATSULayoutOperationSelector  operationSelector;
  ATSUDirectLayoutOperationOverrideUPP  overrideUPP;

};
typedef struct ATSULayoutOperationOverrideSpecifier ATSULayoutOperationOverrideSpecifier;

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

#endif /* __ATSLAYOUTTYPES__ */

