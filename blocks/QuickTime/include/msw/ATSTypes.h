/*
     File:       ATSTypes.h
 
     Contains:   Public interfaces for Apple Type Services components.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ATSTYPES__
#define __ATSTYPES__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
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

typedef UInt32                          FMGeneration;
/* The FMFontFamily reference represents a collection of fonts with the same design
   characteristics. It replaces the standard QuickDraw font identifer and may be used
   with all QuickDraw functions including GetFontName and TextFont. It cannot be used
   with the Resource Manager to access information from a FOND resource handle. A font
   reference does not imply a particular script system, nor is the character encoding
   of a font family determined by an arithmetic mapping of its value.
*/
typedef SInt16                          FMFontFamily;
typedef SInt16                          FMFontStyle;
typedef SInt16                          FMFontSize;
/* 
   The font family is a collection of fonts, each of which is identified
   by an FMFont reference that maps to a single object registered with
   the font database. The font references associated with the font
   family consist of individual outline and bitmapped fonts that may be
   used with the font access routines of the Font Manager and ATS.
*/
typedef UInt32                          FMFont;
struct FMFontFamilyInstance {
  FMFontFamily        fontFamily;
  FMFontStyle         fontStyle;
};
typedef struct FMFontFamilyInstance     FMFontFamilyInstance;
struct FMFontFamilyIterator {
  UInt32              reserved[16];
};
typedef struct FMFontFamilyIterator     FMFontFamilyIterator;
struct FMFontIterator {
  UInt32              reserved[16];
};
typedef struct FMFontIterator           FMFontIterator;
struct FMFontFamilyInstanceIterator {
  UInt32              reserved[16];
};
typedef struct FMFontFamilyInstanceIterator FMFontFamilyInstanceIterator;
enum {
  kInvalidGeneration            = 0L,
  kInvalidFontFamily            = -1,
  kInvalidFont                  = 0L
};

enum {
  kFMCurrentFilterFormat        = 0L
};

typedef UInt32 FMFilterSelector;
enum {
  kFMFontTechnologyFilterSelector = 1L,
  kFMFontContainerFilterSelector = 2L,
  kFMGenerationFilterSelector   = 3L,
  kFMFontFamilyCallbackFilterSelector = 4L,
  kFMFontCallbackFilterSelector = 5L,
  kFMFontDirectoryFilterSelector = 6L
};

enum {
  kFMTrueTypeFontTechnology     = FOUR_CHAR_CODE('true'),
  kFMPostScriptFontTechnology   = FOUR_CHAR_CODE('typ1')
};

typedef CALLBACK_API( OSStatus , FMFontFamilyCallbackFilterProcPtr )(FMFontFamily iFontFamily, void *iRefCon);
typedef CALLBACK_API( OSStatus , FMFontCallbackFilterProcPtr )(FMFont iFont, void *iRefCon);
typedef STACK_UPP_TYPE(FMFontFamilyCallbackFilterProcPtr)       FMFontFamilyCallbackFilterUPP;
typedef STACK_UPP_TYPE(FMFontCallbackFilterProcPtr)             FMFontCallbackFilterUPP;
/*
 *  NewFMFontFamilyCallbackFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( FMFontFamilyCallbackFilterUPP )
NewFMFontFamilyCallbackFilterUPP(FMFontFamilyCallbackFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppFMFontFamilyCallbackFilterProcInfo = 0x000003B0 };  /* pascal 4_bytes Func(2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(FMFontFamilyCallbackFilterUPP) NewFMFontFamilyCallbackFilterUPP(FMFontFamilyCallbackFilterProcPtr userRoutine) { return (FMFontFamilyCallbackFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFMFontFamilyCallbackFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewFMFontFamilyCallbackFilterUPP(userRoutine) (FMFontFamilyCallbackFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFMFontFamilyCallbackFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewFMFontCallbackFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( FMFontCallbackFilterUPP )
NewFMFontCallbackFilterUPP(FMFontCallbackFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppFMFontCallbackFilterProcInfo = 0x000003F0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(FMFontCallbackFilterUPP) NewFMFontCallbackFilterUPP(FMFontCallbackFilterProcPtr userRoutine) { return (FMFontCallbackFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFMFontCallbackFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewFMFontCallbackFilterUPP(userRoutine) (FMFontCallbackFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFMFontCallbackFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeFMFontFamilyCallbackFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeFMFontFamilyCallbackFilterUPP(FMFontFamilyCallbackFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeFMFontFamilyCallbackFilterUPP(FMFontFamilyCallbackFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeFMFontFamilyCallbackFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeFMFontCallbackFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeFMFontCallbackFilterUPP(FMFontCallbackFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeFMFontCallbackFilterUPP(FMFontCallbackFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeFMFontCallbackFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeFMFontFamilyCallbackFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeFMFontFamilyCallbackFilterUPP(
  FMFontFamily                   iFontFamily,
  void *                         iRefCon,
  FMFontFamilyCallbackFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeFMFontFamilyCallbackFilterUPP(FMFontFamily iFontFamily, void * iRefCon, FMFontFamilyCallbackFilterUPP userUPP) { return (OSStatus)CALL_TWO_PARAMETER_UPP(userUPP, uppFMFontFamilyCallbackFilterProcInfo, iFontFamily, iRefCon); }
  #else
    #define InvokeFMFontFamilyCallbackFilterUPP(iFontFamily, iRefCon, userUPP) (OSStatus)CALL_TWO_PARAMETER_UPP((userUPP), uppFMFontFamilyCallbackFilterProcInfo, (iFontFamily), (iRefCon))
  #endif
#endif

/*
 *  InvokeFMFontCallbackFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeFMFontCallbackFilterUPP(
  FMFont                   iFont,
  void *                   iRefCon,
  FMFontCallbackFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeFMFontCallbackFilterUPP(FMFont iFont, void * iRefCon, FMFontCallbackFilterUPP userUPP) { return (OSStatus)CALL_TWO_PARAMETER_UPP(userUPP, uppFMFontCallbackFilterProcInfo, iFont, iRefCon); }
  #else
    #define InvokeFMFontCallbackFilterUPP(iFont, iRefCon, userUPP) (OSStatus)CALL_TWO_PARAMETER_UPP((userUPP), uppFMFontCallbackFilterProcInfo, (iFont), (iRefCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewFMFontFamilyCallbackFilterProc(userRoutine)      NewFMFontFamilyCallbackFilterUPP(userRoutine)
    #define NewFMFontCallbackFilterProc(userRoutine)            NewFMFontCallbackFilterUPP(userRoutine)
    #define CallFMFontFamilyCallbackFilterProc(userRoutine, iFontFamily, iRefCon) InvokeFMFontFamilyCallbackFilterUPP(iFontFamily, iRefCon, userRoutine)
    #define CallFMFontCallbackFilterProc(userRoutine, iFont, iRefCon) InvokeFMFontCallbackFilterUPP(iFont, iRefCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

struct FMFontDirectoryFilter {
  SInt16              fontFolderDomain;
  UInt32              reserved[2];
};
typedef struct FMFontDirectoryFilter    FMFontDirectoryFilter;
struct FMFilter {
  UInt32              format;
  FMFilterSelector    selector;
  union {
    FourCharCode        fontTechnologyFilter;
    FSSpec              fontContainerFilter;
    FMGeneration        generationFilter;
    FMFontFamilyCallbackFilterUPP  fontFamilyCallbackFilter;
    FMFontCallbackFilterUPP  fontCallbackFilter;
    FMFontDirectoryFilter  fontDirectoryFilter;
  }                       filter;
};
typedef struct FMFilter                 FMFilter;

typedef OptionBits                      ATSOptionFlags;
typedef UInt32                          ATSGeneration;
typedef UInt32                          ATSFontContainerRef;
typedef UInt32                          ATSFontFamilyRef;
typedef UInt32                          ATSFontRef;
typedef UInt16                          ATSGlyphRef;
typedef Float32                         ATSFontSize;
enum {
  kATSGenerationUnspecified     = 0L,
  kATSFontContainerRefUnspecified = 0L,
  kATSFontFamilyRefUnspecified  = 0L,
  kATSFontRefUnspecified        = 0L
};

struct ATSFontMetrics {
  UInt32              version;
  Float32             ascent;                 /* Maximum height above baseline reached by the glyphs in the font */
                                              /* or maximum distance to the right of the centerline reached by the glyphs in the font */
  Float32             descent;                /* Maximum depth below baseline reached by the glyphs in the font */
                                              /* or maximum distance to the left of the centerline reached by the glyphs in the font */
  Float32             leading;                /* Desired spacing between lines of text */
  Float32             avgAdvanceWidth;
  Float32             maxAdvanceWidth;        /* Maximum advance width or height of the glyphs in the font */
  Float32             minLeftSideBearing;     /* Minimum left or top side bearing */
  Float32             minRightSideBearing;    /* Minimum right or bottom side bearing */
  Float32             stemWidth;              /* Width of the dominant vertical stems of the glyphs in the font */
  Float32             stemHeight;             /* Vertical width of the dominant horizontal stems of glyphs in the font */
  Float32             capHeight;              /* Height of a capital letter from the baseline to the top of the letter */
  Float32             xHeight;                /* Height of lowercase characters in a font, specifically the letter x, excluding ascenders and descenders */
  Float32             italicAngle;            /* Angle in degrees counterclockwise from the vertical of the dominant vertical strokes of the glyphs in the font */
  Float32             underlinePosition;      /* Distance from the baseline for positioning underlining strokes */
  Float32             underlineThickness;     /* Stroke width for underlining */
};
typedef struct ATSFontMetrics           ATSFontMetrics;
enum {
  kATSItalicQDSkew              = (1 << 16) / 4, /* fixed value of 0.25 */
  kATSBoldQDStretch             = (1 << 16) * 3 / 2, /* fixed value of 1.50 */
  kATSRadiansFactor             = 1144  /* fixed value of approx. pi/180 (0.0174560546875) */
};

/* Glyph outline path constants used in ATSFontGetNativeCurveType. */
typedef UInt16 ATSCurveType;
enum {
  kATSCubicCurveType            = 0x0001,
  kATSQuadCurveType             = 0x0002,
  kATSOtherCurveType            = 0x0003
};

/* 
    This is what the ATSGlyphRef is set to when the glyph is deleted -
    that is, when the glyph is set to no longer appear when the layout
    is actually drawn
*/
enum {
  kATSDeletedGlyphcode          = 0xFFFF
};

struct ATSUCurvePath {
  UInt32              vectors;
  UInt32              controlBits[1];
  Float32Point        vector[1];
};
typedef struct ATSUCurvePath            ATSUCurvePath;
struct ATSUCurvePaths {
  UInt32              contours;
  ATSUCurvePath       contour[1];
};
typedef struct ATSUCurvePaths           ATSUCurvePaths;
/* Glyph ideal metrics */
struct ATSGlyphIdealMetrics {
  Float32Point        advance;
  Float32Point        sideBearing;
  Float32Point        otherSideBearing;
};
typedef struct ATSGlyphIdealMetrics     ATSGlyphIdealMetrics;
/* Glyph screen metrics */
struct ATSGlyphScreenMetrics {
  Float32Point        deviceAdvance;
  Float32Point        topLeft;
  UInt32              height;
  UInt32              width;
  Float32Point        sideBearing;
  Float32Point        otherSideBearing;
};
typedef struct ATSGlyphScreenMetrics    ATSGlyphScreenMetrics;
/* Glyph References */

typedef ATSGlyphRef                     GlyphID;

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

#endif /* __ATSTYPES__ */

